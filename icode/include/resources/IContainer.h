/*
 * IContainer.h
 *
 *  Created on: 31 août 2021
 *      Author: azeddine
 */

#ifndef ICODE_INCLUDE_RESOURCES_ICONTAINER_H_
#define ICODE_INCLUDE_RESOURCES_ICONTAINER_H_
#include "IResource.h"
class IFile;
class IFolder;
class IContainer: public virtual IResource {
public:
	enum {

		/*====================================================================
		 * Constants defining which members are wanted:
		 *====================================================================*/

		/**
		 * Member constant (bit mask value 1) indicating that phantom member resources are
		 * to be included.
		 *
		 * @see IResource#isPhantom()
		 * @since 2.0
		 */
		INCLUDE_PHANTOMS = 1,

		/**
		 * Member constant (bit mask value 2) indicating that team private members are
		 * to be included.
		 *
		 * @see IResource#isTeamPrivateMember()
		 * @since 2.0
		 */
		INCLUDE_TEAM_PRIVATE_MEMBERS = 2,

		/**
		 * Member constant (bit mask value 4) indicating that derived resources
		 * are to be excluded.
		 *
		 * @see IResource#isDerived()
		 * @since 3.1
		 */
		EXCLUDE_DERIVED = 4,

		/**
		 * Member constant (bit mask value 8) indicating that hidden resources
		 * are to be included.
		 *
		 * @see IResource#isHidden()
		 * @since 3.4
		 */
		INCLUDE_HIDDEN = 8,

		/**
		 * Member constant (bit mask value 16) indicating that a resource
		 * should not be checked for existence.
		 *
		 * @see IResource#accept(IResourceProxyVisitor, int)
		 * @see IResource#accept(IResourceVisitor, int, int)
		 * @since 3.8
		 */
		DO_NOT_CHECK_EXISTENCE = 16,
	};

	/**
	 * Returns whether a resource of some type with the given path
	 * exists relative to this resource.
	 * The supplied path may be absolute or relative; in either case, it is
	 * interpreted as relative to this resource.  Trailing separators are ignored.
	 * If the path is empty this container is checked for existence.
	 *
	 * @param path the path of the resource
	 * @return <code>true</code> if a resource of some type with the given path
	 *     exists relative to this resource, and <code>false</code> otherwise
	 * @see IResource#exists()
	 */
	virtual bool Exists(const char *path) = 0;
	/**
	 * Finds and returns the member resource identified by the given path in
	 * this container, or <code>null</code> if no such resource exists.
	 * The supplied path may be absolute or relative; in either case, it is
	 * interpreted as relative to this resource. Trailing separators and the path's
	 * device are ignored. If the path is empty this container is returned. Parent
	 * references in the supplied path are discarded if they go above the workspace
	 * root.
	 * <p>
	 * If the <code>includePhantoms</code> argument is <code>false</code>,
	 * only a member resource with the given path that exists will be returned.
	 * If the <code>includePhantoms</code> argument is <code>true</code>,
	 * the method also returns a resource if the workspace is keeping track of a
	 * phantom with that path.
	 * </p><p>
	 * Note that no attempt is made to exclude team-private member resources
	 * as with <code>members</code>.
	 * </p><p>
	 * N.B. Unlike the methods which traffic strictly in resource
	 * handles, this method infers the resulting resource's type from the
	 * resource (or phantom) existing at the calculated path in the workspace.
	 * </p>
	 *
	 * @param path the path of the desired resource
	 * @param includePhantoms <code>true</code> if phantom resources are
	 *   of interest; <code>false</code> if phantom resources are not of
	 *   interest
	 * @return the member resource, or <code>null</code> if no such
	 * 		resource exists
	 * @see #members(boolean)
	 * @see IResource#isPhantom()
	 */
	virtual IResource* FindMember(const char *path, bool includePhantoms)=0;
	/**
	 * Returns a handle to the file identified by the given path in this
	 * container.
	 * <p>
	 * This is a resource handle operation; neither the resource nor
	 * the result need exist in the workspace.
	 * The validation check on the resource name/path is not done
	 * when the resource handle is constructed; rather, it is done
	 * automatically as the resource is created.
	 * <p>
	 * The supplied path may be absolute or relative; in either case, it is
	 * interpreted as relative to this resource and is appended
	 * to this container's full path to form the full path of the resultant resource.
	 * A trailing separator is ignored. The path of the resulting resource must
	 * have at least two segments.
	 * </p>
	 *
	 * @param path the path of the member file
	 * @return the (handle of the) member file
	 * @see #getFolder(IPath)
	 */
	virtual IFile* GetFile(const char *path)=0;

	/**
	 * Returns a handle to the folder identified by the given path in this
	 * container.
	 * <p>
	 * This is a resource handle operation; neither the resource nor
	 * the result need exist in the workspace.
	 * The validation check on the resource name/path is not done
	 * when the resource handle is constructed; rather, it is done
	 * automatically as the resource is created.
	 * <p>
	 * The supplied path may be absolute or relative; in either case, it is
	 * interpreted as relative to this resource and is appended
	 * to this container's full path to form the full path of the resultant resource.
	 * A trailing separator is ignored. The path of the resulting resource must
	 * have at least two segments.
	 * </p>
	 *
	 * @param path the path of the member folder
	 * @return the (handle of the) member folder
	 * @see #getFile(IPath)
	 */
	virtual IFolder* GetFolder(const char *path)=0;
	/**
	 * Returns a list of all member resources (projects, folders and files)
	 * in this resource, in no particular order.
	 * <p>
	 * If the <code>INCLUDE_PHANTOMS</code> flag is not specified in the member
	 * flags (recommended), only member resources that exist will be returned.
	 * If the <code>INCLUDE_PHANTOMS</code> flag is specified,
	 * the result will also include any phantom member resources the
	 * workspace is keeping track of.
	 * </p><p>
	 * If the <code>INCLUDE_TEAM_PRIVATE_MEMBERS</code> flag is specified
	 * in the member flags, team private members will be included along with
	 * the others. If the <code>INCLUDE_TEAM_PRIVATE_MEMBERS</code> flag
	 * is not specified (recommended), the result will omit any team private
	 * member resources.
	 * </p><p>
	 * If the {@link #INCLUDE_HIDDEN} flag is specified in the member flags, hidden
	 * members will be included along with the others. If the {@link #INCLUDE_HIDDEN} flag
	 * is not specified (recommended), the result will omit any hidden
	 * member resources.
	 * </p>
	 * <p>
	 * If the <code>EXCLUDE_DERIVED</code> flag is not specified, derived
	 * resources are included. If the <code>EXCLUDE_DERIVED</code> flag is
	 * specified in the member flags, derived resources are not included.
	 * </p>
	 *
	 * @param memberFlags bit-wise or of member flag constants
	 *   ({@link #INCLUDE_PHANTOMS}, {@link #INCLUDE_TEAM_PRIVATE_MEMBERS},
	 *   {@link #INCLUDE_HIDDEN} and {@link #EXCLUDE_DERIVED}) indicating which members are of interest
	 * @return an array of members of this resource
	 * @exception CoreException if this request fails. Reasons include:
	 * <ul>
	 * <li> This resource does not exist.</li>
	 * <li> the <code>INCLUDE_PHANTOMS</code> flag is not specified and
	 *     this resource does not exist.</li>
	 * <li> the <code>INCLUDE_PHANTOMS</code> flag is not specified and
	 *     this resource is a project that is not open.</li>
	 * </ul>
	 * @see IResource#exists()
	 * @since 2.0
	 */
	virtual int Members(int memberFlags, IResource **&resources) = 0;
	virtual void ReleaseMembers() = 0;
};
template<>
inline const IID __IID<IContainer>() {
	return IID_IContainer;
}
#endif /* ICODE_INCLUDE_RESOURCES_ICONTAINER_H_ */
