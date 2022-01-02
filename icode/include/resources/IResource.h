/*
 * IResource.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_RESOURCES_IRESOURCE_H_
#define ICODE_RESOURCES_IRESOURCE_H_
#include "IStream.h"
class IContainer;
class IProject;
class IResource: public virtual IObject {
public:
	enum {
		/*====================================================================
		 * Constants defining resource types:  There are four possible resource types
		 * and their type constants are in the integer range 1 to 8 as defined below.
		 *====================================================================*/

		/**
		 * Type constant (bit mask value 1) which identifies file resources.
		 *
		 * @see IResource#getType()
		 * @see IFile
		 */
		FILE = 0x1,

		/**
		 * Type constant (bit mask value 2) which identifies folder resources.
		 *
		 * @see IResource#getType()
		 * @see IFolder
		 */
		FOLDER = 0x2,

		/**
		 * Type constant (bit mask value 4) which identifies project resources.
		 *
		 * @see IResource#getType()
		 * @see IProject
		 */
		PROJECT = 0x4,

		/**
		 * Type constant (bit mask value 8) which identifies the root resource.
		 *
		 * @see IResource#getType()
		 * @see IWorkspaceRoot
		 */
		ROOT = 0x8,

		/*====================================================================
		 * Constants defining the depth of resource tree traversal:
		 *====================================================================*/

		/**
		 * Depth constant (value 0) indicating this resource, but not any of its members.
		 */
		DEPTH_ZERO = 0,

		/**
		 * Depth constant (value 1) indicating this resource and its direct members.
		 */
		DEPTH_ONE = 1,

		/**
		 * Depth constant (value 2) indicating this resource and its direct and
		 * indirect members at any depth.
		 */
		DEPTH_INFINITE = 2,

		/*====================================================================
		 * Constants for update flags for delete, move, copy, open, etc.:
		 *====================================================================*/

		/**
		 * Update flag constant (bit mask value 1) indicating that the operation
		 * should proceed even if the resource is out of sync with the local file
		 * system.
		 *
		 * @since 2.0
		 */
		FORCE = 0x1,

		/**
		 * Update flag constant (bit mask value 2) indicating that the operation
		 * should maintain local history by taking snapshots of the contents of
		 * files just before being overwritten or deleted.
		 *
		 * @see IFile#getHistory(IProgressMonitor)
		 * @since 2.0
		 */
		KEEP_HISTORY = 0x2,

		/**
		 * Update flag constant (bit mask value 4) indicating that the operation
		 * should delete the files and folders of a project.
		 * <p>
		 * Deleting a project that is open ordinarily deletes all its files and folders,
		 * whereas deleting a project that is closed retains its files and folders.
		 * Specifying <code>ALWAYS_DELETE_PROJECT_CONTENT</code> indicates that the contents
		 * of a project are to be deleted regardless of whether the project is open or closed
		 * at the time, specifying <code>NEVER_DELETE_PROJECT_CONTENT</code> indicates that
		 * the contents of a project are to be retained regardless of whether the project
		 * is open or closed at the time.
		 * </p>
		 *
		 * @see #NEVER_DELETE_PROJECT_CONTENT
		 * @since 2.0
		 */
		ALWAYS_DELETE_PROJECT_CONTENT = 0x4,

		/**
		 * Update flag constant (bit mask value 8) indicating that the operation
		 * should preserve the files and folders of a project.
		 * <p>
		 * Deleting a project that is open ordinarily deletes all its files and folders,
		 * whereas deleting a project that is closed retains its files and folders.
		 * Specifying <code>ALWAYS_DELETE_PROJECT_CONTENT</code> indicates that the contents
		 * of a project are to be deleted regardless of whether the project is open or closed
		 * at the time, specifying <code>NEVER_DELETE_PROJECT_CONTENT</code> indicates that
		 * the contents of a project are to be retained regardless of whether the project
		 * is open or closed at the time.
		 * </p>
		 *
		 * @see #ALWAYS_DELETE_PROJECT_CONTENT
		 * @since 2.0
		 */
		NEVER_DELETE_PROJECT_CONTENT = 0x8,

		/**
		 * Update flag constant (bit mask value 16) indicating that the link creation
		 * should proceed even if the local file system file or directory is missing.
		 *
		 * @see IFolder#createLink(IPath, int, IProgressMonitor)
		 * @see IFile#createLink(IPath, int, IProgressMonitor)
		 * @since 2.1
		 */
		ALLOW_MISSING_LOCAL = 0x10,

		/**
		 * Update flag constant (bit mask value 32) indicating that a copy or move
		 * operation should only copy the link, rather than copy the underlying
		 * contents of the linked resource.
		 *
		 * @see #copy(IPath, int, IProgressMonitor)
		 * @see #move(IPath, int, IProgressMonitor)
		 * @since 2.1
		 */
		SHALLOW = 0x20,

		/**
		 * Update flag constant (bit mask value 64) indicating that setting the
		 * project description should not attempt to configure and de-configure
		 * natures.
		 *
		 * @see IProject#setDescription(IProjectDescription, int, IProgressMonitor)
		 * @since 3.0
		 */
		AVOID_NATURE_CONFIG = 0x40,

		/**
		 * Update flag constant (bit mask value 128) indicating that opening a
		 * project for the first time or creating a linked folder should refresh in the
		 * background.
		 *
		 * @see IProject#open(int, IProgressMonitor)
		 * @see IFolder#createLink(URI, int, IProgressMonitor)
		 * @since 3.1
		 */
		BACKGROUND_REFRESH = 0x80,

		/**
		 * Update flag constant (bit mask value 256) indicating that a
		 * resource should be replaced with a resource of the same name
		 * at a different file system location.
		 *
		 * @see IFile#createLink(URI, int, IProgressMonitor)
		 * @see IFolder#createLink(URI, int, IProgressMonitor)
		 * @see IResource#move(IProjectDescription, int, IProgressMonitor)
		 * @since 3.2
		 */
		REPLACE = 0x100,

		/**
		 * Update flag constant (bit mask value 512) indicating that ancestor
		 * resources of the target resource should be checked.
		 *
		 * @see IResource#isLinked(int)
		 * @since 3.2
		 */
		CHECK_ANCESTORS = 0x200,

		/**
		 * Update flag constant (bit mask value 0x400) indicating that a
		 * resource should be marked as derived.
		 *
		 * @see IFile#create(java.io.InputStream, int, IProgressMonitor)
		 * @see IFolder#create(int, boolean, IProgressMonitor)
		 * @see IResource#setDerived(boolean)
		 * @since 3.2
		 */
		DERIVED = 0x400,

		/**
		 * Update flag constant (bit mask value 0x800) indicating that a
		 * resource should be marked as team private.
		 *
		 * @see IFile#create(java.io.InputStream, int, IProgressMonitor)
		 * @see IFolder#create(int, boolean, IProgressMonitor)
		 * @see IResource#copy(IPath, int, IProgressMonitor)
		 * @see IResource#setTeamPrivateMember(boolean)
		 * @since 3.2
		 */
		TEAM_PRIVATE = 0x800,

		/**
		 * Update flag constant (bit mask value 0x1000) indicating that a
		 * resource should be marked as a hidden resource.
		 *
		 * @since 3.4
		 */
		HIDDEN = 0x1000,

		/**
		 * Update flag constant (bit mask value 0x2000) indicating that a
		 * resource should be marked as a virtual resource.
		 *
		 * @see IFolder#create(int, boolean, IProgressMonitor)
		 * @since 3.6
		 */
		VIRTUAL = 0x2000,

		/*====================================================================
		 * Other constants:
		 *====================================================================*/

		/**
		 * Modification stamp constant (value -1) indicating no modification stamp is
		 * available.
		 *
		 * @see #getModificationStamp()
		 */
		NULL_STAMP = -1,

		/**
		 * General purpose zero-valued bit mask constant. Useful whenever you need to
		 * supply a bit mask with no bits set.
		 * <p>
		 * Example usage:
		 * <code>
		 * <pre>
		 *    delete(IResource.NONE, null)
		 * </pre>
		 * </code>
		 * </p>
		 *
		 * @since 2.0
		 */
		NONE = 0,
	};
	/**
	 * Makes a copy of this resource at the given path. The resource's
	 * descendents are copied as well.  The path of this resource must not be a
	 * prefix of the destination path. The workspace root may not be the source or
	 * destination location of a copy operation, and a project can only be copied to
	 * another project. After successful completion, corresponding new resources
	 * will exist at the given path; their contents and properties will be copies of
	 * the originals. The original resources are not affected.
	 * <p>
	 * The supplied path may be absolute or relative.  Absolute paths fully specify
	 * the new location for the resource, including its project. Relative paths are
	 * considered to be relative to the container of the resource being copied. A
	 * trailing separator is ignored.
	 * </p>
	 * <p>
	 * Calling this method with a one segment absolute destination path is
	 * equivalent to calling:
	 * <pre>
	 *   copy(workspace.newProjectDescription(folder.getName()),updateFlags,monitor);
	 * </pre>
	 * </p>
	 * <p> When a resource is copied, its persistent properties are copied with it.
	 * Session properties and markers are not copied.
	 * </p>
	 * <p>
	 * The {@link #FORCE} update flag controls how this method deals with cases
	 * where the workspace is not completely in sync with the local file system. If
	 * {@link #FORCE} is not specified, the method will only attempt to copy
	 * resources that are in sync with the corresponding files and directories in
	 * the local file system; it will fail if it encounters a resource that is out
	 * of sync with the file system. However, if {@link #FORCE} is specified,
	 * the method copies all corresponding files and directories from the local file
	 * system, including ones that have been recently updated or created. Note that
	 * in both settings of the {@link #FORCE} flag, the operation fails if the
	 * newly created resources in the workspace would be out of sync with the local
	 * file system; this ensures files in the file system cannot be accidentally
	 * overwritten.
	 * </p>
	 * <p>
	 * The {@link #SHALLOW} update flag controls how this method deals with linked
	 * resources.  If {@link #SHALLOW} is not specified, then the underlying
	 * contents of the linked resource will always be copied in the file system.  In
	 * this case, the destination of the copy will never be a linked resource or
	 * contain any linked resources.  If {@link #SHALLOW} is specified when a
	 * linked resource is copied into another project, a new linked resource is
	 * created in the destination project that points to the same file system
	 * location.  When a project containing linked resources is copied, the new
	 * project will contain the same linked resources pointing to the same file
	 * system locations.  For both of these shallow cases, no files on disk under
	 * the linked resource are actually copied.  With the {@link #SHALLOW} flag,
	 * copying of linked resources into anything other than a project is not
	 * permitted.  The {@link #SHALLOW} update flag is ignored when copying non-
	 * linked resources.
	 * </p>
	 * <p>
	 * The {@link #DERIVED} update flag indicates that the new resource
	 * should immediately be set as a derived resource.  Specifying this flag
	 * is equivalent to atomically calling {@link #setDerived(boolean)}
	 * with a value of <code>true</code> immediately after creating the resource.
	 * </p>
	 * <p>
	 * The {@link #TEAM_PRIVATE} update flag indicates that the new resource
	 * should immediately be set as a team private resource.  Specifying this flag
	 * is equivalent to atomically calling {@link #setTeamPrivateMember(boolean)}
	 * with a value of <code>true</code> immediately after creating the resource.
	 * </p>
	 * <p>
	 * The {@link #HIDDEN} update flag indicates that the new resource
	 * should immediately be set as a hidden resource.  Specifying this flag
	 * is equivalent to atomically calling {@link #setHidden(boolean)}
	 * with a value of <code>true</code> immediately after creating the resource.
	 * </p>
	 * <p>
	 * Update flags other than those listed above are ignored.
	 * </p>
	 * <p>
	 * This operation changes resources; these changes will be reported in a
	 * subsequent resource change event that will include an indication that the
	 * resource copy has been added to its new parent.
	 * </p>
	 * <p>
	 * An attempt will be made to copy the local history for this resource and its children,
	 * to the destination. Since local history existence is a safety-net mechanism, failure
	 * of this action will not result in automatic failure of the copy operation.
	 * </p>
	 * <p>
	 * This operation is long-running; progress and cancellation are provided
	 * by the given progress monitor.
	 * </p>
	 *
	 * @param destination the destination path
	 * @param updateFlags bit-wise or of update flag constants
	 *   ({@link #FORCE}, {@link #SHALLOW}, {@link #DERIVED}, {@link #TEAM_PRIVATE}, {@link #HIDDEN})
	 * @param monitor a progress monitor, or <code>null</code> if progress
	 *    reporting is not desired
	 * @exception CoreException if this resource could not be copied. Reasons include:
	 * <ul>
	 * <li> This resource does not exist.</li>
	 * <li> This resource or one of its descendents is not local.</li>
	 * <li> The source or destination is the workspace root.</li>
	 * <li> The source is a project but the destination is not.</li>
	 * <li> The destination is a project but the source is not.</li>
	 * <li> The resource corresponding to the parent destination path does not exist.</li>
	 * <li> The resource corresponding to the parent destination path is a closed project.</li>
	 * <li> The source is a linked resource, but the destination is not a project,
	 *      and {@link #SHALLOW} is specified.</li>
	 * <li> A resource at destination path does exist.</li>
	 * <li> This resource or one of its descendants is out of sync with the local file
	 *      system and {@link #FORCE} is not specified.</li>
	 * <li> The workspace and the local file system are out of sync
	 *      at the destination resource or one of its descendants.</li>
	 * <li> The source resource is a file and the destination path specifies a project.</li>
	 * <li> The source is a linked resource, and the destination path does not
	 * 	specify a project.</li>
	 * <li> The location of the source resource on disk is the same or a prefix of
	 * the location of the destination resource on disk.</li>
	 * <li> Resource changes are disallowed during certain types of resource change
	 *       event notification. See <code>IResourceChangeEvent</code> for more details.</li>
	 * </ul>
	 * @exception OperationCanceledException if the operation is canceled.
	 * Cancellation can occur even if no progress monitor is provided.
	 * @see #FORCE
	 * @see #SHALLOW
	 * @see #DERIVED
	 * @see #TEAM_PRIVATE
	 * @see IResourceRuleFactory#copyRule(IResource, IResource)
	 * @since 2.0
	 */
	virtual void Copy(const char *destination, int flags,
			IProgressMonitor *monitor) =0;
	/**
	 * Deletes this resource from the workspace.
	 * Deletion applies recursively to all members of this resource in a "best-
	 * effort" fashion.  That is, all resources which can be deleted are deleted.
	 * Resources which could not be deleted are noted in a thrown exception. The
	 * method does not fail if resources do not exist; it fails only if resources
	 * could not be deleted.
	 * <p>
	 * Deleting a non-linked resource also deletes its contents from the local file
	 * system. In the case of a file or folder resource, the corresponding file or
	 * directory in the local file system is deleted. Deleting an open project
	 * recursively deletes its members; deleting a closed project just gets rid of
	 * the project itself (closed projects have no members); files in the project's
	 * local content area are retained; referenced projects are unaffected.
	 * </p>
	 * <p>
	 * Deleting a linked resource does not delete its contents from the file system,
	 * it just removes that resource and its children from the workspace.  Deleting
	 * children of linked resources does remove the contents from the file system.
	 * </p>
	 * <p>
	 * Deleting a resource also deletes its session and persistent properties and
	 * markers.
	 * </p>
	 * <p>
	 * Deleting a non-project resource which has sync information converts the
	 * resource to a phantom and retains the sync information for future use.
	 * </p>
	 * <p>
	 * Deleting the workspace root resource recursively deletes all projects,
	 * and removes all markers, properties, sync info and other data related to the
	 * workspace root; the root resource itself is not deleted, however.
	 * </p>
	 * <p>
	 * This method changes resources; these changes will be reported
	 * in a subsequent resource change event.
	 * </p>
	 * <p>
	 * This method is long-running; progress and cancellation are provided
	 * by the given progress monitor.
	 * </p>
	 * <p>
	 * The {@link #FORCE} update flag controls how this method deals with
	 * cases where the workspace is not completely in sync with the local
	 * file system. If {@link #FORCE} is not specified, the method will only
	 * attempt to delete files and directories in the local file system that
	 * correspond to, and are in sync with, resources in the workspace; it will fail
	 * if it encounters a file or directory in the file system that is out of sync
	 * with  the workspace.  This option ensures there is no unintended data loss;
	 * it is the recommended setting. However, if {@link #FORCE} is specified,
	 * the method will ruthlessly attempt to delete corresponding files and
	 * directories in the local file system, including ones that have been recently
	 * updated or created.
	 * </p>
	 * <p>
	 * The {@link #KEEP_HISTORY} update flag controls whether or not files that
	 * are about to be deleted from the local file system have their current
	 * contents saved in the workspace's local history. The local history mechanism
	 * serves as a safety net to help the user recover from mistakes that might
	 * otherwise result in data loss. Specifying {@link #KEEP_HISTORY} is
	 * recommended except in circumstances where past states of the files are of no
	 * conceivable interest to the user. Note that local history is maintained
	 * with each individual project, and gets discarded when a project is deleted
	 * from the workspace. Hence {@link #KEEP_HISTORY} is only really applicable
	 * when deleting files and folders, but not projects.
	 * </p>
	 * <p>
	 * The {@link #ALWAYS_DELETE_PROJECT_CONTENT} update flag controls how
	 * project deletions are handled. If {@link #ALWAYS_DELETE_PROJECT_CONTENT}
	 * is specified, then the files and folders in a project's local content area
	 * are deleted, regardless of whether the project is open or closed;
	 * {@link #FORCE} is assumed regardless of whether it is specified. If
	 * {@link #NEVER_DELETE_PROJECT_CONTENT} is specified, then the files and
	 * folders in a project's local content area are retained, regardless of whether
	 * the project is open or closed; the {@link #FORCE} flag is ignored. If
	 * neither of these flags is specified, files and folders in a project's local
	 * content area from open projects (subject to the {@link #FORCE} flag), but
	 * never from closed projects.
	 * </p>
	 *
	 * @param updateFlags bit-wise or of update flag constants (
	 *   {@link #FORCE}, {@link #KEEP_HISTORY},
	 *   {@link #ALWAYS_DELETE_PROJECT_CONTENT},
	 *   and {@link #NEVER_DELETE_PROJECT_CONTENT})
	 * @param monitor a progress monitor, or <code>null</code> if progress
	 *    reporting is not desired
	 * @exception CoreException if this method fails. Reasons include:
	 * <ul>
	 * <li> This resource could not be deleted for some reason.</li>
	 * <li> This resource or one of its descendents is out of sync with the local file system
	 *      and {@link #FORCE} is not specified.</li>
	 * <li> Resource changes are disallowed during certain types of resource change
	 *       event notification. See <code>IResourceChangeEvent</code> for more details.</li>
	 * </ul>
	 * @exception OperationCanceledException if the operation is canceled.
	 * Cancellation can occur even if no progress monitor is provided.
	 * @see IFile#delete(boolean, boolean, IProgressMonitor)
	 * @see IFolder#delete(boolean, boolean, IProgressMonitor)
	 * @see #FORCE
	 * @see #KEEP_HISTORY
	 * @see #ALWAYS_DELETE_PROJECT_CONTENT
	 * @see #NEVER_DELETE_PROJECT_CONTENT
	 * @see IResourceRuleFactory#deleteRule(IResource)
	 * @since 2.0
	 */
	virtual void Delete(int updateFlags, IProgressMonitor *monitor) =0;
	/**
	 * Returns whether this resource exists in the workspace.
	 * <p>
	 * <code>IResource</code> objects are lightweight handle objects
	 * used to access resources in the workspace. However, having a
	 * handle object does not necessarily mean the workspace really
	 * has such a resource. When the workspace does have a genuine
	 * resource of a matching type, the resource is said to
	 * <em>exist</em>, and this method returns <code>true</code>;
	 * in all other cases, this method returns <code>false</code>.
	 * In particular, it returns <code>false</code> if the workspace
	 * has no resource at that path, or if it has a resource at that
	 * path with a type different from the type of this resource handle.
	 * </p>
	 * <p>
	 * Note that no resources ever exist under a project
	 * that is closed; opening a project may bring some
	 * resources into existence.
	 * </p>
	 * <p>
	 * The name and path of a resource handle may be invalid.
	 * However, validation checks are done automatically as a
	 * resource is created; this means that any resource that exists
	 * can be safely assumed to have a valid name and path.
	 * </p>
	 *
	 * @return <code>true</code> if the resource exists, otherwise
	 *    <code>false</code>
	 */
	virtual bool Exists() = 0;

	/**
	 * Returns the file extension portion of this resource's name,
	 * or <code>null</code> if it does not have one.
	 * <p>
	 * The file extension portion is defined as the string
	 * following the last period (".") character in the name.
	 * If there is no period in the name, the path has no
	 * file extension portion. If the name ends in a period,
	 * the file extension portion is the empty string.
	 * </p>
	 * <p>
	 * This is a resource handle operation; the resource need
	 * not exist.
	 * </p>
	 *
	 * @return a string file extension
	 * @see #getName()
	 */
	const char* GetFileExtension() {
		const char *s = GetFullPath();
		const char *t = strrchr(s, '.');
		if (t == 0)
			return 0;
		else
			return t++;
	}

	/**
	 * Returns the full, absolute path of this resource relative to the
	 * workspace.
	 * <p>
	 * This is a resource handle operation; the resource need
	 * not exist.
	 * If this resource does exist, its path can be safely assumed to be valid.
	 * </p>
	 * <p>
	 * A resource's full path indicates the route from the root of the workspace
	 * to the resource.  Within a workspace, there is exactly one such path
	 * for any given resource. The first segment of these paths name a project;
	 * remaining segments, folders and/or files within that project.
	 * The returned path never has a trailing separator.  The path of the
	 * workspace root is <code>Path.ROOT</code>.
	 * </p>
	 * <p>
	 * Since absolute paths contain the name of the project, they are
	 * vulnerable when the project is renamed. For most situations,
	 * project-relative paths are recommended over absolute paths.
	 * </p>
	 *
	 * @return the absolute path of this resource
	 * @see #getProjectRelativePath()
	 * @see Path#ROOT
	 */
	virtual const char* GetFullPath() = 0;
	/**
	 * Returns the absolute path in the local file system to this resource,
	 * or <code>null</code> if no path can be determined.
	 * <p>
	 * If this resource is the workspace root, this method returns
	 * the absolute local file system path of the platform working area.
	 * </p><p>
	 * If this resource is a project that exists in the workspace, this method
	 * returns the path to the project's local content area. This is true regardless
	 * of whether the project is open or closed. This value will be null in the case
	 * where the location is relative to an undefined workspace path variable.
	 * </p><p>
	 * If this resource is a linked resource under a project that is open, this
	 * method returns the resolved path to the linked resource's local contents.
	 * This value will be null in the case where the location is relative to an
	 * undefined workspace path variable.
	 * </p><p>
	 * If this resource is a file or folder under a project that exists, or a
	 * linked resource under a closed project, this method returns a (non-
	 * <code>null</code>) path computed from the location of the project's local
	 * content area and the project- relative path of the file or folder. This is
	 * true regardless of whether the file or folders exists, or whether the project
	 * is open or closed. In the case of linked resources, the location of a linked resource
	 * within a closed project is too computed from the location of the
	 * project's local content area and the project-relative path of the resource. If the
	 * linked resource resides in an open project then its location is computed
	 * according to the link.
	 * </p><p>
	 * If this resource is a project that does not exist in the workspace,
	 * or a file or folder below such a project, this method returns
	 * <code>null</code>.  This method also returns <code>null</code> if called
	 * on a resource that is not stored in the local file system.  For such resources
	 * {@link #getLocationURI()} should be used instead.
	 * </p>
	 *
	 * @return the absolute path of this resource in the local file system,
	 *  or <code>null</code> if no path can be determined
	 * @see #getRawLocation()
	 * @see #getLocationURI()
	 * @see  IProjectDescription#setLocation(IPath)
	 * @see Platform#getLocation()
	 */
	virtual const char* GetLocation() = 0;

	/**
	 * Returns the absolute URI of this resource,
	 * or <code>null</code> if no URI can be determined.
	 * <p>
	 * If this resource is the workspace root, this method returns
	 * the absolute location of the platform working area.
	 * </p><p>
	 * If this resource is a project that exists in the workspace, this method
	 * returns the URI to the project's local content area. This is true regardless
	 * of whether the project is open or closed. This value will be null in the case
	 * where the location is relative to an undefined workspace path variable.
	 * </p><p>
	 * If this resource is a linked resource under a project that is open, this
	 * method returns the resolved URI to the linked resource's local contents.
	 * This value will be null in the case where the location is relative to an
	 * undefined workspace path variable.
	 * </p><p>
	 * If this resource is a file or folder under a project that exists, or a
	 * linked resource under a closed project, this method returns a (non-
	 * <code>null</code>) URI computed from the location of the project's local
	 * content area and the project- relative path of the file or folder. This is
	 * true regardless of whether the file or folders exists, or whether the project
	 * is open or closed. In the case of linked resources, the location of a linked resource
	 * within a closed project is computed from the location of the
	 * project's local content area and the project-relative path of the resource. If the
	 * linked resource resides in an open project then its location is computed
	 * according to the link.
	 * </p><p>
	 * If this resource is a project that does not exist in the workspace,
	 * or a file or folder below such a project, this method returns
	 * <code>null</code>.
	 * </p>
	 *
	 * @return the absolute URI of this resource,
	 *  or <code>null</code> if no URI can be determined
	 * @see #getRawLocation()
	 * @see  IProjectDescription#setLocation(IPath)
	 * @see Platform#getLocation()
	 * @see java.net.URI
	 * @since 3.2
	 */
	virtual const char* GetLocationURI()=0;

	/**
	 * Returns the name of this resource.
	 * The name of a resource is synonymous with the last segment
	 * of its full (or project-relative) path for all resources other than the
	 * workspace root.  The workspace root's name is the empty string.
	 * <p>
	 * This is a resource handle operation; the resource need
	 * not exist.
	 * </p>
	 * <p>
	 * If this resource exists, its name can be safely assumed to be valid.
	 * </p>
	 *
	 * @return the name of the resource
	 * @see #getFullPath()
	 * @see #getProjectRelativePath()
	 */
	inline const char* GetName() {
		const char *s = GetFullPath();
		const char *t = strrchr(s, '/');
		if (t == 0)
			return s;
		else
			return t + 1;
	}
	/**
	 * Returns the resource which is the parent of this resource,
	 * or <code>null</code> if it has no parent (that is, this
	 * resource is the workspace root).
	 * <p>
	 * The full path of the parent resource is the same as this
	 * resource's full path with the last segment removed.
	 * </p>
	 * <p>
	 * This is a resource handle operation; neither the resource
	 * nor the resulting resource need exist.
	 * </p>
	 *
	 * @return the parent resource of this resource,
	 *    or <code>null</code> if it has no parent
	 */
	virtual IContainer* GetParent()=0;
	/**
	 * Returns the project which contains this resource.
	 * Returns itself for projects and <code>null</code>
	 * for the workspace root.
	 * <p>
	 * A resource's project is the one named by the first segment
	 * of its full path.
	 * </p>
	 * <p>
	 * This is a resource handle operation; neither the resource
	 * nor the resulting project need exist.
	 * </p>
	 *
	 * @return the project handle
	 */
	virtual IProject* GetProject()=0;

	/**
	 * Returns a relative path of this resource with respect to its project.
	 * Returns the empty path for projects and the workspace root.
	 * <p>
	 * This is a resource handle operation; the resource need not exist.
	 * If this resource does exist, its path can be safely assumed to be valid.
	 * </p>
	 * <p>
	 * A resource's project-relative path indicates the route from the project
	 * to the resource.  Within a project, there is exactly one such path
	 * for any given resource. The returned path never has a trailing slash.
	 * </p>
	 * <p>
	 * Project-relative paths are recommended over absolute paths, since
	 * the former are not affected if the project is renamed.
	 * </p>
	 *
	 * @return the relative path of this resource with respect to its project
	 * @see #getFullPath()
	 * @see #getProject()
	 * @see Path#EMPTY
	 */
	virtual const char* GetProjectRelativePath()=0;
	/**
	 * Gets this resource's extended attributes from the file system,
	 * or <code>null</code> if the attributes could not be obtained.
	 * <p>
	 * Reasons for a <code>null</code> return value include:
	 * <ul>
	 * <li> This resource does not exist.</li>
	 * <li> This resource is not local.</li>
	 * <li> This resource is a project that is not open.</li>
	 * </ul>
	 * </p><p>
	 * Attributes that are not supported by the underlying file system
	 * will have a value of <code>false</code>.
	 * </p><p>
	 * Sample usage: <br>
	 * <br>
	 * <code>
	 *  IResource resource; <br>
	 *  ... <br>
	 *  ResourceAttributes attributes = resource.getResourceAttributes(); <br>
	 *  if (attributes != null) {
	 *     attributes.setExecutable(true); <br>
	 *     resource.setResourceAttributes(attributes); <br>
	 *  }
	 * </code>
	 * </p>
	 *
	 * @return the extended attributes from the file system, or
	 * <code>null</code> if they could not be obtained
	 * @see #setResourceAttributes(ResourceAttributes)
	 * @see ResourceAttributes
	 * @since 3.1
	 */
	virtual int GetResourceAttributes(int mask)=0;
	/**
	 * Returns the type of this resource.
	 * The returned value will be one of {@link #FILE}, {@link #FOLDER}, {@link #PROJECT}, {@link #ROOT}.
	 * <p>
	 * <ul>
	 * <li> All resources of type {@link #FILE} implement {@link IFile}.</li>
	 * <li> All resources of type {@link #FOLDER} implement {@link IFolder}.</li>
	 * <li> All resources of type {@link #PROJECT} implement {@link IProject}.</li>
	 * <li> All resources of type {@link #ROOT} implement {@link IWorkspaceRoot}.</li>
	 * </ul>
	 * </p>
	 * <p>
	 * This is a resource handle operation; the resource need not exist in the workspace.
	 * </p>
	 *
	 * @return the type of this resource
	 * @see #FILE
	 * @see #FOLDER
	 * @see #PROJECT
	 * @see #ROOT
	 */
	virtual int GetType()=0;
	/**
	 * Returns whether this resource is accessible.  For files and folders,
	 * this is equivalent to existing; for projects,
	 * this is equivalent to existing and being open.  The workspace root
	 * is always accessible.
	 *
	 * @return <code>true</code> if this resource is accessible, and <code>false</code> otherwise
	 * @see #exists()
	 * @see IProject#isOpen()
	 */
	bool IsAccessible() {

	}

	/**
	 * Returns whether this resource subtree is marked as derived. Returns
	 * <code>false</code> if this resource does not exist.
	 *
	 * <p>
	 * This is a convenience method,
	 * fully equivalent to <code>isDerived(IResource.NONE)</code>.
	 * </p>
	 *
	 * @return <code>true</code> if this resource is marked as derived, and
	 *     <code>false</code> otherwise
	 * @see #setDerived(boolean)
	 * @since 2.0
	 */
	bool IsDerived() {

	}

	/**
	 * Returns whether this resource subtree is marked as derived. Returns
	 * <code>false</code> if this resource does not exist.
	 *
	 * <p>
	 * The {@link #CHECK_ANCESTORS} option flag indicates whether this method
	 * should consider ancestor resources in its calculation.  If the
	 * {@link #CHECK_ANCESTORS} flag is present, this method will return
	 * <code>true</code>, if this resource, or any parent resource, is marked
	 * as derived.  If the {@link #CHECK_ANCESTORS} option flag is not specified,
	 * this method returns false for children of derived resources.
	 * </p>
	 *
	 * @param options bit-wise or of option flag constants (only {@link #CHECK_ANCESTORS} is applicable)
	 * @return <code>true</code> if this resource subtree is derived, and <code>false</code> otherwise
	 * @see IResource#setDerived(boolean)
	 * @since 3.4
	 */
	bool IsDerivedCheckAncestors() {

	}

	/**
	 * Returns whether this resource is hidden in the resource tree. Returns
	 * <code>false</code> if this resource does not exist.
	 * <p>
	 * This operation is not related to the file system hidden attribute accessible using
	 * {@link ResourceAttributes#isHidden()}.
	 * </p>
	 *
	 * @return <code>true</code> if this resource is hidden, and <code>false</code> otherwise
	 * @see #setHidden(boolean)
	 * @since 3.4
	 */
	bool IsHidden() {

	}

	/**
	 * Returns whether this resource is hidden in the resource tree. Returns
	 * <code>false</code> if this resource does not exist.
	 * <p>
	 * This operation is not related to the file system hidden attribute
	 * accessible using {@link ResourceAttributes#isHidden()}.
	 * </p>
	 * <p>
	 * The {@link #CHECK_ANCESTORS} option flag indicates whether this method
	 * should consider ancestor resources in its calculation. If the
	 * {@link #CHECK_ANCESTORS} flag is present, this method will return
	 * <code>true</code> if this resource, or any parent resource, is a hidden
	 * resource. If the {@link #CHECK_ANCESTORS} option flag is not specified,
	 * this method returns false for children of hidden resources.
	 * </p>
	 *
	 * @param options
	 *        bit-wise or of option flag constants (only
	 *        {@link #CHECK_ANCESTORS} is applicable)
	 * @return <code>true</code> if this resource is hidden , and
	 *         <code>false</code> otherwise
	 * @see #setHidden(boolean)
	 * @since 3.5
	 */
	bool IsHiddenCheckAncestors() {

	}

	/**
	 * Returns whether this resource has been linked to
	 * a location other than the default location calculated by the platform.
	 * <p>
	 * 	 This is a convenience method, fully equivalent to
	 * <code>isLinked(IResource.NONE)</code>.
	 * </p>
	 *
	 * @return <code>true</code> if this resource is linked, and
	 *   <code>false</code> otherwise
	 * @see IFile#createLink(IPath, int, IProgressMonitor)
	 * @see IFolder#createLink(IPath, int, IProgressMonitor)
	 * @since 2.1
	 */
	bool IsLinked() {

	}

	/**
	 * Returns whether this resource is a virtual resource. Returns <code>true</code>
	 * for folders that have been marked virtual using the {@link #VIRTUAL} update
	 * flag.  Returns <code>false</code> in all other cases, including
	 * the case where this resource does not exist.  The workspace root, projects
	 * and files currently cannot be made virtual.
	 *
	 * @return <code>true</code> if this resource is virtual, and
	 *         <code>false</code> otherwise
	 * @see IFile#create(java.io.InputStream, int, IProgressMonitor)
	 * @see #VIRTUAL
	 * @since 3.6
	 */
	bool IsVirtual() {

	}

	/**
	 * Returns <code>true</code> if this resource has been linked to
	 * a location other than the default location calculated by the platform. This
	 * location can be outside the project's content area or another location
	 * within the project. Returns <code>false</code> in all other cases, including
	 * the case where this resource does not exist.  The workspace root and
	 * projects are never linked.
	 * <p>
	 * This method returns true for a resource that has been linked using
	 * the <code>createLink</code> method.
	 * </p>
	 * <p>
	 * The {@link #CHECK_ANCESTORS} option flag indicates whether this method
	 * should consider ancestor resources in its calculation.  If the
	 * {@link #CHECK_ANCESTORS} flag is present, this method will return
	 * <code>true</code> if this resource, or any parent resource, is a linked
	 * resource.  If the {@link #CHECK_ANCESTORS} option flag is not specified,
	 * this method returns false for children of linked resources.
	 * </p>
	 *
	 * @param options bit-wise or of option flag constants
	 *  (only {@link #CHECK_ANCESTORS} is applicable)
	 * @return <code>true</code> if this resource is linked, and
	 *   <code>false</code> otherwise
	 * @see IFile#createLink(IPath, int, IProgressMonitor)
	 * @see IFolder#createLink(IPath, int, IProgressMonitor)
	 * @since 3.2
	 */
	bool IsLinkedCheckAncestors() {

	}
	/**
	 * Returns whether this resource is a phantom resource.
	 * <p>
	 * The workspace uses phantom resources to remember outgoing deletions and
	 * incoming additions relative to an external synchronization partner.  Phantoms
	 * appear and disappear automatically as a byproduct of synchronization.
	 * Since the workspace root cannot be synchronized in this way, it is never a phantom.
	 * Projects are also never phantoms.
	 * </p>
	 * <p>
	 * The key point is that phantom resources do not exist (in the technical
	 * sense of <code>exists</code>, which returns <code>false</code>
	 * for phantoms) are therefore invisible except through a handful of
	 * phantom-enabled API methods (notably <code>IContainer.members(boolean)</code>).
	 * </p>
	 *
	 * @return <code>true</code> if this resource is a phantom resource, and
	 *   <code>false</code> otherwise
	 * @see #exists()
	 * @see IContainer#members(boolean)
	 * @see IContainer#findMember(String, boolean)
	 * @see IContainer#findMember(IPath, boolean)
	 * @see ISynchronizer
	 */
	bool IsPhantom() {

	}

	/**
	 * Returns whether this resource is marked as read-only in the file system.
	 *
	 * @return <code>true</code> if this resource is read-only,
	 *		<code>false</code> otherwise
	 * @deprecated use {@link #getResourceAttributes()}
	 */
	bool IsReadOnly() {

	}

	/**
	 * Returns whether this resource and its descendents to the given depth
	 * are considered to be in sync with the local file system.
	 * <p>
	 * A resource is considered to be in sync if all of the following
	 * conditions are true:
	 * <ul>
	 * <li>The resource exists in both the workspace and the file system.</li>
	 * <li>The timestamp in the file system has not changed since the
	 * last synchronization.</li>
	 * <li>The resource in the workspace is of the same type as the corresponding
	 * file in the file system (they are either both files or both folders).</li>
	 * </ul>
	 *  A resource is also considered to be in sync if it is missing from both
	 * the workspace and the file system.  In all other cases the resource is
	 * considered to be out of sync.
	 * </p>
	 * <p>
	 * This operation interrogates files and folders in the local file system;
	 * depending on the speed of the local file system and the requested depth,
	 * this operation may be time-consuming.
	 * </p>
	 *
	 * @param depth the depth (one of {@link #DEPTH_ZERO}, {@link #DEPTH_ONE}, or {@link #DEPTH_INFINITE})
	 * @return <code>true</code> if this resource and its descendents to the
	 *    specified depth are synchronized, and <code>false</code> in all other
	 *    cases
	 * @see IResource#DEPTH_ZERO
	 * @see IResource#DEPTH_ONE
	 * @see IResource#DEPTH_INFINITE
	 * @see #refreshLocal(int, IProgressMonitor)
	 * @since 2.0
	 */
	bool IsSynchronized(int depth) {

	}
	/**
	 * Moves this resource so that it is located at the given path.
	 * The path of the resource must not be a prefix of the destination path. The
	 * workspace root may not be the source or destination location of a move
	 * operation, and a project can only be moved to another project. After
	 * successful completion, the resource and any direct or indirect members will
	 * no longer exist; but corresponding new resources will now exist at the given
	 * path.
	 * <p>
	 * The supplied path may be absolute or relative.  Absolute paths fully specify
	 * the new location for the resource, including its project.  Relative paths are
	 * considered to be relative to the container of the resource being moved. A
	 * trailing slash is ignored.
	 * </p>
	 * <p>
	 * Calling this method with a one segment absolute destination path is
	 * equivalent to calling:
	 * <pre>
	 IProjectDescription description = getDescription();
	 description.setName(path.lastSegment());
	 move(description, updateFlags, monitor);
	 * </pre>
	 * </p>
	 * <p> When a resource moves, its session and persistent properties move with
	 * it. Likewise for all other attributes of the resource including markers.
	 * </p>
	 * <p>
	 * The {@link #FORCE} update flag controls how this method deals with cases
	 * where the workspace is not completely in sync with the local file system. If
	 * {@link #FORCE} is not specified, the method will only attempt to move
	 * resources that are in sync with the corresponding files and directories in
	 * the local file system; it will fail if it encounters a resource that is out
	 * of sync with the file system. However, if {@link #FORCE} is specified,
	 * the method moves all corresponding files and directories from the local file
	 * system, including ones that have been recently updated or created. Note that
	 * in both settings of the {@link #FORCE} flag, the operation fails if the
	 * newly created resources in the workspace would be out of sync with the local
	 * file system; this ensures files in the file system cannot be accidentally
	 * overwritten.
	 * </p>
	 * <p>
	 * The {@link #KEEP_HISTORY} update flag controls whether or not
	 * file that are about to be deleted from the local file system have their
	 * current contents saved in the workspace's local history. The local history
	 * mechanism serves as a safety net to help the user recover from mistakes that
	 * might otherwise result in data loss. Specifying {@link #KEEP_HISTORY}
	 * is recommended except in circumstances where past states of the files are of
	 * no conceivable interest to the user. Note that local history is maintained
	 * with each individual project, and gets discarded when a project is deleted
	 * from the workspace. Hence {@link #KEEP_HISTORY} is only really applicable
	 * when moving files and folders, but not whole projects.
	 * </p>
	 * <p>
	 * If this resource is not a project, an attempt will be made to copy the local history
	 * for this resource and its children, to the destination. Since local history existence
	 * is a safety-net mechanism, failure of this action will not result in automatic failure
	 * of the move operation.
	 * </p>
	 * <p>
	 * The {@link #SHALLOW} update flag controls how this method deals with linked
	 * resources.  If {@link #SHALLOW} is not specified, then the underlying
	 * contents of the linked resource will always be moved in the file system.  In
	 * this case, the destination of the move will never be a linked resource or
	 * contain any linked resources. If {@link #SHALLOW} is specified when a
	 * linked resource is moved into another project, a new linked resource is
	 * created in the destination project that points to the same file system
	 * location.  When a project containing linked resources is moved, the new
	 * project will contain the same linked resources pointing to the same file
	 * system locations.  For either of these cases, no files on disk under the
	 * linked resource are actually moved. With the {@link #SHALLOW} flag,
	 * moving of linked resources into anything other than a project is not
	 * permitted.  The {@link #SHALLOW} update flag is ignored when moving non-
	 * linked resources.
	 * </p>
	 * <p>
	 * Update flags other than {@link #FORCE}, {@link #KEEP_HISTORY}and
	 * {@link #SHALLOW} are ignored.
	 * </p>
	 * <p>
	 * This method changes resources; these changes will be reported in a subsequent
	 * resource change event that will include an indication that the resource has
	 * been removed from its parent and that a corresponding resource has been added
	 * to its new parent. Additional information provided with resource delta shows
	 * that these additions and removals are related.
	 * </p>
	 * <p>
	 * This method is long-running; progress and cancellation are provided
	 * by the given progress monitor.
	 * </p>
	 *
	 * @param destination the destination path
	 * @param updateFlags bit-wise or of update flag constants
	 *   ({@link #FORCE}, {@link #KEEP_HISTORY} and {@link #SHALLOW})
	 * @param monitor a progress monitor, or <code>null</code> if progress
	 *    reporting is not desired
	 * @exception CoreException if this resource could not be moved. Reasons include:
	 * <ul>
	 * <li> This resource does not exist.</li>
	 * <li> This resource or one of its descendents is not local.</li>
	 * <li> The source or destination is the workspace root.</li>
	 * <li> The source is a project but the destination is not.</li>
	 * <li> The destination is a project but the source is not.</li>
	 * <li> The resource corresponding to the parent destination path does not exist.</li>
	 * <li> The resource corresponding to the parent destination path is a closed
	 *      project.</li>
	 * <li> The source is a linked resource, but the destination is not a project
	 *      and  {@link #SHALLOW} is specified.</li>
	 * <li> A resource at destination path does exist.</li>
	 * <li> A resource of a different type exists at the destination path.</li>
	 * <li> This resource or one of its descendents is out of sync with the local file system
	 *      and <code>force</code> is <code>false</code>.</li>
	 * <li> The workspace and the local file system are out of sync
	 *      at the destination resource or one of its descendents.</li>
	 * <li> The source resource is a file and the destination path specifies a project.</li>
	 * <li> The location of the source resource on disk is the same or a prefix of
	 * the location of the destination resource on disk.</li>
	 * <li> Resource changes are disallowed during certain types of resource change
	 * event notification. See <code>IResourceChangeEvent</code> for more details.</li>
	 * </ul>
	 * @exception OperationCanceledException if the operation is canceled.
	 * Cancellation can occur even if no progress monitor is provided.
	 * @see IResourceDelta#getFlags()
	 * @see #FORCE
	 * @see #KEEP_HISTORY
	 * @see #SHALLOW
	 * @see IResourceRuleFactory#moveRule(IResource, IResource)
	 * @since  2.0
	 */
	virtual void Move(const char *destination, int updateFlags,
			IProgressMonitor *monitor)=0;
	/**
	 * Refreshes the resource hierarchy from this resource and its
	 * children (to the specified depth) relative to the local file system.
	 * Creations, deletions, and changes detected in the local file system
	 * will be reflected in the workspace's resource tree.
	 * This resource need not exist or be local.
	 * <p>
	 * This method may discover changes to resources; any such
	 * changes will be reported in a subsequent resource change event.
	 * </p>
	 * <p>
	 * If a new file or directory is discovered in the local file
	 * system at or below the location of this resource,
	 * any parent folders required to contain the new
	 * resource in the workspace will also be created automatically as required.
	 * </p>
	 * <p>
	 * This method is long-running; progress and cancellation are provided
	 * by the given progress monitor.
	 * </p>
	 *
	 * @param depth valid values are {@link #DEPTH_ZERO}, {@link #DEPTH_ONE}, or {@link #DEPTH_INFINITE}
	 * @param monitor a progress monitor, or <code>null</code> if progress reporting is not desired
	 * @exception CoreException if this method fails. Reasons include:
	 * <ul>
	 * <li> Resource changes are disallowed during certain types of resource change
	 *       event notification. See <code>IResourceChangeEvent</code> for more details.</li>
	 * </ul>
	 * @exception OperationCanceledException if the operation is canceled.
	 * Cancellation can occur even if no progress monitor is provided.
	 * @see IResource#DEPTH_ZERO
	 * @see IResource#DEPTH_ONE
	 * @see IResource#DEPTH_INFINITE
	 * @see IResourceRuleFactory#refreshRule(IResource)
	 */
	virtual void RefreshLocal(int depth, IProgressMonitor *monitor)=0;
	/**
	 * Sets whether this resource subtree is marked as derived.
	 * <p>
	 * A <b>derived</b> resource is a regular file or folder that is
	 * created in the course of translating, compiling, copying, or otherwise
	 * processing other files. Derived resources are not original data, and can be
	 * recreated from other resources. It is commonplace to exclude derived
	 * resources from version and configuration management because they would
	 * otherwise clutter the team repository with version of these ever-changing
	 * files as each user regenerates them.
	 * </p>
	 * <p>
	 * If a resource or any of its ancestors is marked as derived, a team
	 * provider should assume that the resource is not under version and
	 * configuration management <i>by default</i>. That is, the resource
	 * should only be stored in a team repository if the user explicitly indicates
	 * that this resource is worth saving.
	 * </p>
	 * <p>
	 * Newly-created resources are not marked as derived; rather, the mark must be
	 * set explicitly using <code>setDerived(true, IProgressMonitor)</code>. Derived marks are maintained
	 * in the in-memory resource tree, and are discarded when the resources are deleted.
	 * Derived marks are saved to disk when a project is closed, or when the workspace
	 * is saved.
	 * </p>
	 * <p>
	 * Projects and the workspace root are never considered derived; attempts to
	 * mark them as derived are ignored.
	 * </p>
	 * <p>
	 * These changes will be reported in a subsequent resource change event,
	 * including an indication that this file's derived flag has changed.
	 * </p>
	 * <p>
	 * This method is long-running; progress and cancellation are provided
	 * by the given progress monitor.
	 * </p>
	 *
	 * @param isDerived <code>true</code> if this resource is to be marked
	 * 		as derived, and <code>false</code> otherwise
	 * @param monitor a progress monitor, or <code>null</code> if progress
	 * 		reporting is not desired
	 * @exception OperationCanceledException if the operation is canceled.
	 * 		Cancellation can occur even if no progress monitor is provided.
	 * @exception CoreException if this method fails. Reasons include:
	 * <ul>
	 * <li> This resource does not exist.</li>
	 * <li> Resource changes are disallowed during certain types of resource change
	 *       event notification. See <code>IResourceChangeEvent</code> for more details.</li>
	 * </ul>
	 * @see #isDerived()
	 * @see IResourceRuleFactory#derivedRule(IResource)
	 * @since 3.6
	 */
	inline void SetDerived(bool isDerived, IProgressMonitor *monitor) {

	}
	/**
	 * Sets whether this resource and its members are hidden in the resource tree.
	 * <p>
	 * Hidden resources are invisible to most clients. Newly-created resources
	 * are not hidden resources by default.
	 * </p>
	 * <p>
	 * The workspace root is never considered hidden resource;
	 * attempts to mark it as hidden are ignored.
	 * </p>
	 * <p>
	 * This operation does <b>not</b> result in a resource change event, and does not
	 * trigger autobuilds.
	 * </p>
	 * <p>
	 * This operation is not related to {@link ResourceAttributes#setHidden(boolean)}.
	 * Whether a resource is hidden in the resource tree is unrelated to whether the
	 * underlying file is hidden in the file system.
	 * </p>
	 *
	 * @param isHidden <code>true</code> if this resource is to be marked
	 *   as hidden, and <code>false</code> otherwise
	 * @exception CoreException if this method fails. Reasons include:
	 * <ul>
	 * <li> This resource does not exist.</li>
	 * <li> Resource changes are disallowed during certain types of resource change
	 *       event notification. See <code>IResourceChangeEvent</code> for more details.</li>
	 * </ul>
	 * @see #isHidden()
	 * @since 3.4
	 */
	void SetHidden(bool isHidden, IProgressMonitor *monitor) {

	}
	/**
	 * Sets or unsets this resource as read-only in the file system.
	 *
	 * @param readOnly <code>true</code> to set it to read-only,
	 *		<code>false</code> to unset
	 * @deprecated use <tt>IResource#setResourceAttributes(ResourceAttributes)</tt>
	 */
	inline void SetReadOnly(bool readOnly, IProgressMonitor *monitor) {

	}

	/**
	 * Sets this resource with the given extended attributes. This sets the
	 * attributes in the file system. Only attributes that are supported by
	 * the underlying file system will be set.
	 * <p>
	 * Sample usage: <br>
	 * <br>
	 * <code>
	 *  IResource resource; <br>
	 *  ... <br>
	 *  if (attributes != null) {
	 *     attributes.setExecutable(true); <br>
	 *     resource.setResourceAttributes(attributes); <br>
	 *  }
	 * </code>
	 * </p>
	 * <p>
	 * Note that a resource cannot be converted into a symbolic link by
	 * setting resource attributes with {@link ResourceAttributes#isSymbolicLink()}
	 * set to true.
	 * </p>
	 *
	 * @param attributes the attributes to set
	 * @exception CoreException if this method fails. Reasons include:
	 * <ul>
	 * <li> This resource does not exist.</li>
	 * <li> This resource is not local.</li>
	 * <li> This resource is a project that is not open.</li>
	 * </ul>
	 * @see #getResourceAttributes()
	 * @since 3.1
	 */
	virtual int SetResourceAttributes(int attributes, int mask,
			IProgressMonitor *monitor)=0;
};
template<>
inline const IID __IID<IResource>() {
	return IID_IResource;
}
#endif /* ICODE_INCLUDE_RESOURCES_IRESOURCE_H_ */
